from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

package_name = 'autoaim_alpha'
def generate_launch_description():
    return LaunchDescription([
        Node(
            package=package_name,
            node_executable='node_observer',
            node_name='node_observer',
            output='log'
        ),
        Node(
            package=package_name,
            node_executable='node_decision_maker',
            node_name='node_decision_maker',
            output='log'
        ),
        Node(
            package='autoaim_beta',
            node_executable='node_webcam',
            node_name='node_webcam',
            output='log'
        ),
        Node(
            package=package_name,
            node_executable='node_com',
            node_name='node_com',
            output='log'
        ),
        Node(
            package=package_name,
            node_executable='node_monitor',
            node_name='node_monitor',
            output='log'
        )
    ])
